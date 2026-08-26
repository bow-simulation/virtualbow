# Introduction

Welcome to the VirtualBow user manual!
This guide will show you how to design bows, simulate their performance, and interpret the results using the VirtualBow software.
The chapters follow the order in which you will typically use them: [Model Editor](model-editor.md) for creating a bow, [Result Viewer](result-viewer.md) for interpreting the simulation results, and [Solver](solver.md) for more advanced use cases.
The [Appendix](appendix.md) collects practical tips and a bending test for determining material properties, while the [Changelog](changelog.md) records what changed in each release.
For support, feedback, or to download the latest version of the software, visit the project's website:

[**https://www.virtualbow.org/**](https://www.virtualbow.org/)

Please keep in mind that VirtualBow is still under active development, and many of its simulation results have not yet been systematically validated.
So the results should not be taken at face value just yet, even though users often report good agreement already.
Generally, three main factors influence how closely a simulation can match an actual bow:

**Physical simplifications:** Every simulation relies on simplifying assumptions that make complex real-world behavior mathematically manageable. VirtualBow tries to make reasonable assumptions, but no model can capture reality perfectly. If you're interested in the details of the underlying theory, have a look at the [VirtualBow Theory Manual [PDF]](https://www.virtualbow.org/files/theory-manual.pdf).

**Software implementation:** Errors in the software's code can affect accuracy as well.
And even though much work has been put into verifying the simulation code through automated testing, bugs can never be completely ruled out.

**Quality of input data:** A simulation is only as accurate as the provided input.
Material properties or other parameters might not be known for certain due to a lack of data, natural variations, or other reasons.
Also, in practice no bow can be built exactly as it has been modeled in VirtualBow.
Even small deviations in geometry, for example, can accumulate and affect the outcome.

Many of these points are being worked on to further improve the accuracy of VirtualBow.
And while simulations will never be perfect, they can still reveal a lot about how a bow design works, highlight weaknesses, and point you toward possible improvements.
So think of VirtualBow as a complementary tool to real-world experimentation: it won't replace building and testing real bows, but it can make the process more informed and efficient.