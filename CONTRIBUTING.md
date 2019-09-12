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
* **Provide specific examples to demonstrate the steps**. Include links to files or GitHub projects, or licenses, which can cause the bug. If you're providing code snippets in the issue, use [Markdown code blocks](https://help.github.com/articles/markdown-basics/#multiple-lines). The best way to report a bug is to **design a test** to demonstrate it. 
* **If you're reporting that Open License Manager crashed**, include a crash dump and the associated message. 
* **Label the issue as bug.**

Provide more context by answering these questions:

* **Can you reproduce the problem using the example application?**
* **Can you reliably reproduce the issue?** If not, provide details about how often the problem happens and under which conditions it normally happens.
* If the problem is related integrating Open License Manager with your application, **produce a minimal example to demonstrate it** Does the problem happen only with some license type? Does the problem only happen in Linux/Docker/Windows?

Include details about your configuration and environment:

* **Update Open License Manager to the latest version** If possible try to pull the latest changes from `develop` branch.
* **What's the name and version of the OS you're using**?
* **What's the name and version of the compiler you're using**? Are you cross compiling?
* **What's are the `cmake` command line you used to generate your build scripts **? 
* **Are you running Open License Manager in a virtual machine/docker?** If so, which VM software are you using and which operating systems and versions are used for the guest?

### Suggesting Enhancements

This section guides you through submitting an enhancement suggestion for Open License Manager, including completely new features and minor improvements to existing functionality. Following these guidelines helps maintainers and the community understand your suggestion :pencil: and find related suggestions :mag_right:.

Before creating enhancement suggestions, please check [existing enhancement request](https://github.com/open-license-manager/open-license-manager/issues?utf8=%E2%9C%93&q=is%3Aissue+label%3Aenhancement) to see if the enhancement has already been suggested, as you might find out that you don't need to create one. When you are creating an enhancement suggestion, please [include as many details as possible](#how-do-i-submit-a-good-enhancement-suggestion). 

Please also check for the [current and planned features](https://github.com/open-license-manager/open-license-manager/wiki/features) in the wiki to see where the project is heading to.

#### How Do I Submit A (Good) Enhancement Suggestion?

Enhancement suggestions are tracked as [GitHub issues](https://guides.github.com/features/issues/). 

* **Use a clear and descriptive title** for the issue to identify the suggestion.
* **Provide a detailed description of the suggested enhancement** in as many details as possible.
* **Provide specific examples to demonstrate the steps**. Include copy/pasteable snippets of code which we could use, or reference to other libraries, algorithms, open source code.
* **Describe the current behavior** and **explain which behavior you expected to see instead** and why.
* **Explain why this enhancement would be useful** to most Open License Manager users and isn't something that can or should be implemented.
* **List some other software protection where this enhancement exists.**
* **Specify the name and version of the OS you're proposing the enhancement for.** If applicable include all the environment informations: is this for running the application in a VM? in a Docker?
* **Label the issue as enhancement.**

### Your First Code Contribution

Unsure where to begin contributing to Open License Manager? You can start by looking through the [`good first issue`](https://github.com/open-license-manager/open-license-manager/issues?utf8=%E2%9C%93&q=is%3Aissue+is%3Aopen+label%3A%22good+first+issue%22) and [`help-wanted`](https://github.com/open-license-manager/open-license-manager/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) issues. 

You can have a look to the project upcoming milestones to see what's going to be implemented soon and what is the project direction. If you decide to contribute on an issue scheduled in the current milestone, comment on the issue first. If the issue is marked as "assigned" there may be already some work done for it. Commenting on the issue will ensure we don't duplicate our work. Also have a look to the branches:  there may be a feature branch corresponding to the issue with some work already done.   

If you have already forked the repository to implement a specific feature, and you want your code to be merged in the main repository please first file an enhancement request as explained in [suggesting enhancements](#suggesting-enhancements). If you found a bug and you want to propose a fix please [report a bug](#reporting-bugs) before.

We apply a subset of [GitFlow](https://nvie.com/posts/a-successful-git-branching-model) development workflow. Be sure to work against `develop` branch, since `master` is reserved for stable releases, and may be outdated.
 
#### General coding rules

Supposing you already know how to contribute to an open source project on GitHub (if you have doubts you can check this short [guide](https://git-scm.com/book/en/v2/GitHub-Contributing-to-a-Project) ), you're working on an existing issue the code is already committed on your fork. 

 * Ensure your feature branch is up to date with the `develop`, eventually merge the latest changes from the `develop` branch. This will help us save time.
 * ~~ Reformat the changed code using "clang-format" to keep consistent formatting style ~~ (not yet).
 * Prepare your pull request, in the pull request comment reference the issue the pull request will fix.
 * Check your pull request compiles and pass the checks on Travis CI
 * In the pull request comment reference the issue you want to fix.

##### Don't
 * Don't reformat the code following your personal likes, it introduce a lot of "noise" and makes very hard to merge. 
 * Very large pull requests with few comments, no corresponding issue will probably be rejected.
    * We understand that the project is still in a very alpha stage and a rearrangement is needed, however we would like to discuss it with you before we take project changing decision. 
    * We can't break current functionality, user established habits without documenting the change.

  