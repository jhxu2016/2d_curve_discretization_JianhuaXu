
#include "sysconfig.h"
#include "curve2d.h"

#include <algorithm>
#include <cmath>

//sampling point p
typedef struct
{
	double t; //point parameter
	Point2D pt; //cache of the coordinates, prevent duplicate calculation
} IntPoint;

template <typename function> static void RecursiveSampling(Curve2D* pCurve, 
	IntPoint* from, IntPoint* to, function f, 
	std::vector<IntPoint>& vecPoints, int& depth)
{
	IntPoint mid;
	depth++;
	mid.t = 0.5 * (from->t + to->t);
	mid.pt = f(pCurve, mid.t);

	//calculate acc
	std::vector<Point2D> points({ from->pt, mid.pt, to->pt });
	double dStartVel, dEndVel, dMaxAcc;
	Curve2D::calcVelocityAcc(points, dStartVel, dEndVel, dMaxAcc);
	//do not go very deep, prevent inifinite recursion
	if (dMaxAcc <= sysconfig::m_gAccThreshold || depth > 50)
	{
		vecPoints.push_back(mid);
		vecPoints.push_back(*to);
	}
	else
	{
		RecursiveSampling(pCurve, from, &mid, f, vecPoints, depth);
		RecursiveSampling(pCurve, &mid, to, f, vecPoints, depth);
	}
	depth--;
}

//
template <typename function> static void pointSampling(Curve2D* pCurve, 
	double t1, double t2, function f, std::vector<IntPoint>& vecPoints)
{
	IntPoint from, to;
	int depth = 0;
	from.t = t1;
	from.pt = f(pCurve, from.t);
	vecPoints.push_back(from);
	to.t = t2;
	to.pt = f(pCurve, to.t);
	RecursiveSampling(pCurve, &from, &to, f, vecPoints, depth);
}

//for point sampling
struct getCurvePoint
{
	Point2D operator()(Curve2D* pCurve, double t)
	{
		Point2D pt = pCurve->evalCurve(t);
		return std::move(pt);
	}
};

/////////////////////////////
/// \brief Curve2D::evalCurve
/// \param vecParams
/// \return

void Curve2D::calcVelocityAcc(const std::vector<Point2D>& vecPoints,
	double& dStartVel, double& dEndVel,
	double& dMaxAcc)
{
	std::vector<Point2D> vecFirstDiffs(vecPoints.size() - 1, Point2D(0., 0.));
	for (auto i = 0; i < vecPoints.size() - 1; i++)
	{
		vecFirstDiffs[i].setX(vecPoints[i + 1].getX() - vecPoints[i].getX());
		vecFirstDiffs[i].setY(vecPoints[i + 1].getY() - vecPoints[i].getY());
	}
	std::vector<double> vecVels = normFrobenius(vecFirstDiffs);
	dStartVel = vecVels[0];
	dEndVel = vecVels.back();

	//calculate acc
	std::vector<Point2D> vecSecondDiffs(vecFirstDiffs.size() - 1, Point2D(0., 0.));
	for (auto i = 0; i < vecFirstDiffs.size() - 1; i++)
	{
		vecSecondDiffs[i].setX(vecFirstDiffs[i + 1].getX() - vecFirstDiffs[i].getX());
		vecSecondDiffs[i].setY(vecFirstDiffs[i + 1].getY() - vecFirstDiffs[i].getY());
	}
	std::vector<double> vecAccs = normFrobenius(vecSecondDiffs);
	auto maxIt = std::max_element(vecAccs.begin(), vecAccs.end());
	dMaxAcc = vecAccs[std::distance(vecAccs.begin(), maxIt)];
}

std::vector<Point2D> Curve2D::evalCurve(const std::vector<double>& vecParams)
{
	std::vector<Point2D> vecPoints(vecParams.size(), Point2D(0., 0.));
	for (size_t i = 0; i < vecParams.size(); i++)
		vecPoints[i] = this->evalCurve(vecParams[i]);
	return std::move(vecPoints);
}

std::vector<double> Curve2D::normFrobenius(const std::vector<Point2D>& vecPoints)
{
	std::vector<double> norms(vecPoints.size());
	for (auto i = 0; i < vecPoints.size(); i++)
	{
		const Point2D& pt = vecPoints[i];
		double d = pt.getX() * pt.getX() + pt.getY() * pt.getY();
		d = std::sqrt(d);
		norms[i] = d;
	}
	return std::move(norms);
}

///
//core function for curve discretization algorithm
std::vector<double> Curve2D::discretizeCurve()
{
	std::vector<double> vecParams;
	std::vector<IntPoint> vecPoints;
	pointSampling(this, 0., 1., getCurvePoint(), vecPoints);
	//start and end vel should be 0
	if (!vecPoints.empty())
		vecParams.push_back(vecPoints[0].t);
	for (const auto& pt2d : vecPoints)
		vecParams.push_back(pt2d.t);
	vecParams.push_back(vecParams.back());
	return std::move(vecParams);
}

//evalute the 2d point for a straight line
Point2D Straight::evalCurve(const double t)
{
	Point2D pt;
	pt.setX(m_ptStart.getX() + t * (m_ptEnd.getX() - m_ptStart.getX()));
	pt.setY(m_ptStart.getY() + t * (m_ptEnd.getY() - m_ptStart.getY()));
	return std::move(pt);
}

//evalute the 2d points for a ellipse
Point2D Ellipse::evalCurve(const double t)
{
	Point2D pt;
	pt.setX(m_ptCenter.getX() + m_dRadiusX * std::cos(t));
	pt.setY(m_ptCenter.getY() + m_dRadiusY * std::sin(t));
	return std::move(pt);
}
