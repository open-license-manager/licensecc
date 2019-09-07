Contributing to Open License Manager
==========
:+1::tada: First off, if you landed here means you want to contribute! :tada::+1:

The following is a set of guidelines for contributing to Open License Manager and its packages, which are hosted in the [Open License Manager organization](https://github.com/open-license-manager) on GitHub. These are mostly guidelines, not rules. Use your best judgment, and feel free to propose changes to this document in a pull request.

## How Can I Contribute?

### Reporting Bugs

This section guides you through submitting a bug report for Open License Manager. Following these guidelines helps maintainers and the community understand your report :pencil:, reproduce the behavior :computer: :computer:, and find related reports :mag_right:.

Before creating bug reports, please [check the repository](https://github.com/open-license-manager/open-license-manager/issues) to see if the problem has already been reported. If it has **and the issue is still open**, add a comment to the existing issue instead of opening a new one. When you are creating a bug report, please [include as many details as possible](#how-do-i-submit-a-good-bug-report). 

#### How Do I Submit A (Good) Bug Report?

Bugs are tracked as [GitHub issues](https://guides.github.com/features/issues/). Explain the problem and include additional details to help maintainers reproduce the problem:

* **Use a clear and descriptive title** for the issue to identify the problem.
* **Describe the exact steps which reproduce the problem** in as many details as possible. For example, start by explaining how are you using Open License Manager. 
* **Provide specific examples to demonstrate the steps**. Include links to files or GitHub projects, or copy/pasteable snippets, which you use in those examples. If you're providing snippets in the issue, use [Markdown code blocks](https://help.github.com/articles/markdown-basics/#multiple-lines).
* **Describe the behavior you observed after following the steps** and point out what exactly is the problem with that behavior.
* **Explain which behavior you expected to see instead and why.**
* **If you're reporting that Open License Manager crashed**, include a crash dump and the associated message. 
* **Label the issue as bug.**

Provide more context by answering these questions:

* **Can you reproduce the problem using the example application?**
* **Can you reliably reproduce the issue?** If not, provide details about how often the problem happens and under which conditions it normally happens.
* If the problem is related integrating Open License Manager with your application, **produce a minimal example to demonstrate it** Does the problem happen only with some license type? Does the problem only happen in Linux/Docker/Windows?

Include details about your configuration and environment:

* **Update Open License Manager to the latest version** If possible try to pull the latest changes from master branch.
* **What's the name and version of the OS you're using**?
* **What's the name and version of the compiler you're using**? Are you cross compiling?
* **Are you running Open License Manager in a virtual machine/docker?** If so, which VM software are you using and which operating systems and versions are used for the host and the guest?

### Suggesting Enhancements

This section guides you through submitting an enhancement suggestion for Open License Manager, including completely new features and minor improvements to existing functionality. Following these guidelines helps maintainers and the community understand your suggestion :pencil: and find related suggestions :mag_right:.


Before creating enhancement suggestions, please check [existing enhancement request](https://github.com/open-license-manager/open-license-manager/issues?utf8=%E2%9C%93&q=is%3Aissue+label%3Aenhancement) to see if the enhancement has already been suggested, as you might find out that you don't need to create one. When you are creating an enhancement suggestion, please [include as many details as possible](#how-do-i-submit-a-good-enhancement-suggestion). 

#### How Do I Submit A (Good) Enhancement Suggestion?

Enhancement suggestions are tracked as [GitHub issues](https://guides.github.com/features/issues/). 

* **Use a clear and descriptive title** for the issue to identify the suggestion.
* **Provide a detailed description of the suggested enhancement** in as many details as possible.
* **Provide specific examples to demonstrate the steps**. Include copy/pasteable snippets of code which we could use, or reference to other libraries, algorithms, open source code.
* **Describe the current behavior** and **explain which behavior you expected to see instead** and why.
* **Explain why this enhancement would be useful** to most Open License Manager users and isn't something that can or should be implemented.
* **List some other software protection where this enhancement exists.**
* **Specify the name and version of the OS you're proposing the enhancement for.** If applicable include all the environment informations: is this for running the application in a VM? in a Docker?
* **Specify the version of the .**
* **Label the issue as enhancement.**

### Your First Code Contribution

Unsure where to begin contributing to Open License Manager? You can start by looking through the `[good first issue](https://github.com/open-license-manager/open-license-manager/issues?utf8=%E2%9C%93&q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22)` and `[help-wanted](https://github.com/open-license-manager/open-license-manager/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22)` issues. 

You can have a look to the project upcoming milestones to see what's going to be implemented soon. If you decide to contribute on an issue scheduled in next milestone, comment on the issue first. If the issue is already "assigned" means there is already some work done for it we may have not pushed it yet.   

If you have already forked the repository to implement a specific feature, and you want your code to be merged in the main repository please first file an enhancement request as explained in [suggesting enhancements](#suggesting-enhancements). If you found a bug and you want to propose a fix please [report a bug](#reporting-bugs) before.

#### General coding rules

Supposing you already know how to contribute to an open source project on GitHub (if you have doubts you can check this short [guide](https://git-scm.com/book/en/v2/GitHub-Contributing-to-a-Project) ), you're working on an existing issue the code is already committed on your fork. 

 * Ensure your feature branch is up to date with the master, eventually merge the latest changes from the master. This will help us save time in the merge.
 * ~~ Reformat the changed code using "clang-format" to keep consistent formatting style ~~ (not yet).
 * Prepare your pull request, in the pull request comment reference the issue the pull request will fix.
 * Check your pull request compiles and pass the tests on Travis CI
 * Comment on the issue you want to fix.

##### Don't
 * Don't reformat the code following your personal likes, it introduce a lot of "noise" and makes very hard to merge. 
 * Very large pull requests with few comments, no corresponding issue will probably be rejected.
    * We understand that the project is still in a very alpha stage and a rearrangement is possible and needed, however we would like to discuss it with you before we take project changing decision. 
    * We can't break current functionality, user established habits without documenting the change.

#Funny facts 

### Project general context (is the project still maintained?)

This project started as a week-end project, donated and almost abandoned. Thanks to the effort of some people that are using it, and to the help of many of you it has been kept alive (2019).  

The most challenging aspect is the number of environment/libraries we must work with. We're trying to set up automated tests on Travis CI to help contributors to verify their code automatically in conditions they've not thought about. 
 

### Open License Manager and "the community"

Open License Manager has a very strange relationship with "the community", its aim is to protect proprietary software from unintended usage. But out there, there are people that think that all the software should be licensed as AGPL... Thus StackOverflow and its fellow sites turned out to be not a collaborative place to ask questions about software protection. Try if you want, but to choose your words carefully. :speak_no_evil: