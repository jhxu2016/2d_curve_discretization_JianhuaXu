# I'm using: python=3.8 numpy=1.21.5 matplotlib=3.5.1, but this should work with whatever...
import numpy as np
import matplotlib.pyplot as plt


def eval_curve(eval_pts: np.ndarray, plot_points=False):
    """Parameterization of a spiral

    Args:
        eval_pts (np.ndarray): Numpy array of length N corresponding to parameters 
        to be evaluated along a curve.

    Returns:
        points (np.ndarray): Nx2 numpy array of 2d point data. 
    """
    assert isinstance(eval_pts, np.ndarray), "Input must be numpy array"

    radius = eval_pts * 3 + 1
    x_vals = np.cos(np.pi - eval_pts * np.pi) * radius
    y_vals = np.sin(eval_pts * np.pi) * radius
    points = np.stack((x_vals, y_vals)).T

    if plot_points:
        plt.plot(*points.T, '-o')
        plt.show()

    return points

def assess_answer(eval_pts: np.ndarray):
    """Given an array of parameters, tells you how well you did

    Args:
        eval_pts (np.ndarray): Requirements:
            All values must be between 0 and 1. 
            You gotta start at 0 and end with 1
            You need to start and end at velocity=0
            Max acceleration must be below threshold (0.1)

    Prints:
        some cheeky assessment of eval_pts

    """
    assert min(eval_pts) >= 0, "Cannot evaluate negative evaluation point"
    assert max(eval_pts) <= 1, "Cannot evaluate evaluation point > 1"
    assert eval_pts[0] == 0. and eval_pts[-1] == 1., "Eval_pts must start with 0 and end with 1"
    acc_threshold = 0.1

    points = eval_curve(eval_pts)
    vel = np.linalg.norm(np.diff(points, axis=0), axis=1)
    vel_start = vel[0]
    vel_final = vel[-1]
    # Here is how we measure discrete acceleration:
    acc = np.linalg.norm(np.diff(np.diff(points, axis=0), axis=0), axis=1)
    max_acc = max(acc)

    assert max_acc <= acc_threshold, f"{max_acc=} exceeds {acc_threshold=}!"
    assert np.allclose(vel_start, 0), f"You gotta start at zero velocity, not: {vel_start}"
    assert np.allclose(vel_final, 0), f"You gotta end at zero velocity, not: {vel_final}"

    score = len(points)
    print(f"You used {score} points... that's... ")
    if score > 100:
        print("In the ballpark of the example code. We can go faster!")
    elif score > 38:
        print("Pretty nice! You are well on your way!")
    elif score > 25:
        print("Exceptional! You should be very proud!")
    elif score == 25:
        print("Optimal! (So far as I know...)")
    else:
        print("Better than what I thought possible?! I'd love to hear how you did this!")

