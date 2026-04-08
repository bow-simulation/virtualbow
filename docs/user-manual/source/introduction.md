# Introduction

Welcome to the VirtualBow user manual!
This guide will show you how to use the VirtualBow software to design bows, simulate their performance, and interpret the results.
For support, feedback, or to download the latest version, please visit the project website: [https://www.virtualbow.org/](https://www.virtualbow.org/).

Please keep in mind that VirtualBow is still under active development, and many of its simulation results have not yet been systematically validated.
Although users often report good agreement with real-world behavior, the results should not be taken at face value.
Generally, three main factors influence how closely the simulation can match an actual bow:

* **Physical simplifications:** Every simulation relies on assumptions that make complex real-world behavior mathematically manageable. VirtualBow tries to make reasonable assumptions, but no model can capture reality perfectly. If you're interested in the underlying theory, you may want to explore the *VirtualBow theory manual*.

* **Software implementation:** Errors in the code can obviously affect accuracy. While much work has been put into verifying the simulation code by automated testing, bugs can never be completely ruled out.

* **Quality of input data:** A simulation is only as accurate as the data provided.
In practice, no bow can be built exactly as modeled in VirtualBow, even though the type of bow can make this task easier or harder. Natural variations in material properties and small deviations in geometry, for example, will accumulate and affect the outcome.

Many of those points are being worked on to further improve the accuracy of VirtualBow.
And while simulations will never be perfect, they can still reveal a lot about how a bow design works, highlight weaknesses, and point you toward possible improvements.
Think of VirtualBow as a complementary tool to real world experimentation: it won't replace building and testing real bows, but it can make the process more informed and efficient.
