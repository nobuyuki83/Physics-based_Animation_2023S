# Task05: Dynamic Mass-spring System using Variational Euler Time Integration

![preview](preview.png)

**Deadline: June 8th (Thu) at 15:00pm**

----

## Before Doing Assignment

If you have not done the [task00](../task00), do it first to set up the C++ graphics development environment.

Follow [this document](../doc/submit.md) to submit the assignment, In a nutshell, before doing the assignment,

- install `eigen` library following  [this document](../doc/setup_eigen.md)
- make sure you synchronized the `main ` branch of your local repository  to that of remote repository.
- make sure you created branch `task06` from `main` branch.
- make sure you are currently in the `task06` branch (use `git branch -a` command).

Now you are ready to go!

---

## Problem 1

Run the code as it is .  After running the program for a while, the simulation stops. Then, take a screenshot image of a program window. 
Save the screenshot image overwriting `task06/problem1.png`

![problem1](problem1.png)


## Problem 2

This program means to compute dynamic animation of skirt as a mass-spring system. The program uses the variational Euler's method to compute implicit time integration. The variational problem is solved using the Newton's method.  

The current code does not compute the hessian of the elastic energy of the spring very accurately so the simulation is unstable.

Write a few lines of code around `line #52` to compute hessian accurately. After running the program for a while, save the screenshot image overwriting `task06/problem2.png`

![problem2](problem2.png)

Write down the converged minimum value of the energy by updating the text below.

## After Doing the Assignment

After modifying the code, push the code and submit a pull request.




## Notes

- Do not submit multiple pull requests. Only the first pull request is graded
- Do not close the pull request by yourself. The instructor will close the pull request
- If you mistakenly merge the pull request, it's OK, but be careful not to merge next time. 
