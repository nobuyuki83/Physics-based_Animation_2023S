# Task01: Implicit Time Integration

![preview](preview.png)

**Deadline: Apr. 20th (Thu) at 15:00pm**

----

## Before Doing Assignment

If you have not done the [task00](../task00), do it first to set up the C++ graphics development environment.

Follow [this document](../doc/submit.md) to submit the assignment, In a nutshell, before doing the assignment,

- install `eigen` library following  [this document](../doc/setup_eigen.md)
- make sure you synchronized the `main ` branch of your local repository  to that of remote repository.
- make sure you created branch `task01` from `main` branch.
- make sure you are currently in the `task01` branch (use `git branch -a` command).

Now you are ready to go!

---

## Problem 1

1. Build the code using `cmake`
2. Run the code
3. Take a screenshot image (looks like image at the top)
4. Save the screenshot image overwriting `task01/problem1.png`

![problem1](problem1.png)


## Problem 2

This program solves the equation of motion ${d^2 r}/{d t^2} = - {1}/{r^2}$ and visualize it in the polar coordinate.

Modify the code at `line #33`  and `line #34` in [main.cpp](main.cpp) to integrate the equation of motion.
Specifically, use the Euler's semi-implicit scheme (linearization of right-hand side) for the integration.

Save the screenshot image overwriting `task01/problem2.png`

![problem2](problem2.png)


## After Doing the Assignment

After modify the code, push the code and submit a pull request.
