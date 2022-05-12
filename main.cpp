
#include "sysconfig.h"
#include "curve2d.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

void outputLog(const std::string& sMessage)
{
	if (!sMessage.empty())
		std::cout << sMessage << std::endl;
}

void assessAnswer(Curve2D* pCurve,
	const double dAccThreshold = sysconfig::m_gAccThreshold)
{
	//log messages
	std::string sMessage("");
	if (!pCurve)
	{
		sMessage = "No input curve.\n";
		outputLog(sMessage);
		return;
	}

	std::vector<double> vecParams = pCurve->discretizeCurve();
	if (vecParams.size() < 2)
	{
		sMessage = "Empty parameters.\n";
		outputLog(sMessage);
		return;
	}

	//most likely it's not necessary
	std::sort(vecParams.begin(), vecParams.end());
	if (vecParams[0] < 0)
	{
		sMessage = "Cannot evaluate negative evaluation point\n";
		outputLog(sMessage);
		return;
	}
	if (vecParams.back() > 1)
	{
		sMessage = "Cannot evaluate evaluation point > 1\n";
		outputLog(sMessage);
		return;
	}

	//epsilon?
	if (vecParams[0] != 0 || vecParams.back() != 1.)
	{
		sMessage = "Eval_pts must start with 0 and end with 1\n";
		outputLog(sMessage);
		return;
	}

	std::vector<double> vecTempParams;
	std::copy(vecParams.begin() + 1, vecParams.end(), std::back_inserter(vecTempParams));
	vecTempParams.pop_back();
	std::vector<Point2D> vecPoints = pCurve->evalCurve(vecTempParams);
	double dStartVel, dEndVel, dMaxAcc;
	Curve2D::calcVelocityAcc(vecPoints, dStartVel, dEndVel, dMaxAcc);
	if (dMaxAcc > dAccThreshold)
	{
		sMessage = "{max_acc=} exceeds {acc_threshold=}!\n";
		outputLog(sMessage);
		return;
	}

	//no need check this because the first and the last are duplicated
#if 0
	if (std::fabs(dStartVel) > std::numeric_limits<double>::epsilon())
	{
		sMessage = "You gotta start at zero velocity, not: {vel_start}\n";
		outputLog(sMessage);
		return;
	}
	if (std::fabs(dEndVel) > std::numeric_limits<double>::epsilon())
	{
		sMessage = "You gotta end at zero velocity, not: {vel_final}\n";
		outputLog(sMessage);
		return;
	}
#endif

	auto nScore = vecPoints.size();
	sMessage = "You used " + std::to_string(nScore) + " points... that's... \n";
	outputLog(sMessage);
	if (nScore > 100)
		sMessage = "In the ballpark of the example code. We can go faster!\n";
	else if (nScore > 38)
		sMessage = "Pretty nice! You are well on your way!\n";
	else if (nScore > 25)
		sMessage = "Exceptional! You should be very proud!\n";
	else if (nScore == 25)
		sMessage = "Optimal! (So far as I know...)\n";
	else
		sMessage = "Better than what I thought possible?! I'd love to hear how you did this!\n";
	outputLog(sMessage);
}

int main(int argc, char* argv[])
{
	Point2D ptStart(0, 0), ptEnd(1, 1);
	auto pStraight = std::shared_ptr<Straight>(new Straight(ptStart, ptEnd));
	assessAnswer(pStraight.get());
	Point2D ptCenter(1.0, 1.0);
	double dRadiusX = 3, dRadiusY = 2;
	std::shared_ptr<Ellipse> pEllipse(new Ellipse(ptCenter, dRadiusX, dRadiusY));
	assessAnswer(pEllipse.get());
	dRadiusX = 5, dRadiusY = 5;
	std::shared_ptr<Ellipse> pCircle(new Ellipse(ptCenter, dRadiusX, dRadiusY));
	assessAnswer(pCircle.get());
	dRadiusX = 10000, dRadiusY = 20000;
	std::shared_ptr<Ellipse> pEllipse1(new Ellipse(ptCenter, dRadiusX, dRadiusY));
	assessAnswer(pEllipse1.get());
	return 0;
}
