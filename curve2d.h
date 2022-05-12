#ifndef CURVE2D_H
#define CURVE2D_H

#include <vector>

//an auxiliary class for 2D point
class Point2D
{
public:
    Point2D() = default;
    Point2D(double x, double y) : m_dX(x), m_dY(y) {}
    ~Point2D() = default;
    inline double getX() const { return m_dX; }
    inline double getY() const { return m_dY; }
    inline void setX(double x) { m_dX = x; }
    inline void setY(double y) { m_dY = y; }

private:
    double m_dX;
    double m_dY;
};

class Curve2D
{
public:
    Curve2D() = default;
    virtual ~Curve2D() = default;
    virtual std::vector<double> discretizeCurve();
    virtual Point2D evalCurve(const double t) = 0;
    virtual std::vector<Point2D> evalCurve(const std::vector<double>& vecParams);
    static std::vector<double> normFrobenius(const std::vector<Point2D>& vecPoints);
    static void calcVelocityAcc(const std::vector<Point2D>& vecPoints, double& dStartVel, double& dEndVel, double& dMaxAcc);
};

class Straight : public Curve2D
{
public:
    Straight() = default;
    Straight(const Point2D& ptStart, const Point2D& ptEnd) 
    : m_ptStart(ptStart), m_ptEnd(ptEnd) {}
    virtual ~Straight() = default;
    virtual Point2D evalCurve(const double t);
private:
    Point2D m_ptStart;
    Point2D m_ptEnd;
};

//Circle is a special case of Ellipse
class Ellipse : public Curve2D
{
public:
    Ellipse() = default;
    Ellipse(const Point2D& center, double rX, double rY) 
    : m_ptCenter(center), m_dRadiusX(rX), m_dRadiusY(rY) {}
    virtual ~Ellipse() = default;
    virtual Point2D evalCurve(const double t);

private:
    Point2D m_ptCenter;
    double m_dRadiusX;
    double m_dRadiusY;
};

class Parabola : public Curve2D
{
public:
    Parabola() = default;
    virtual ~Parabola() = default;
    virtual Point2D evalCurve(const double t);
};

class Hyperbola : public Curve2D
{
public:
    Hyperbola() = default;
    virtual ~Hyperbola() = default;
    virtual Point2D evalCurve(const double t);
};

#endif // CURVE2D_H
