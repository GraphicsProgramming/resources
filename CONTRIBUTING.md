# Contributing

Want to contribute to the resources on the repository? Great.  First and foremost, all contributions should be adherent to the community code of conduct.

## Creating Resources

Any resource created must be written in English.  Resources with code must state in the first section of content what language(s) the code is written in.  The languages used must be consistent across all content, e.g. do not mix Java and C# syntax.  All resources should be written as if the reader has a fundamental understanding of the topic area.  For more complex topics, include a section titled `Read Before` to give the reader a reference to fundamental materials.  Include a section called `Prerequisites` as needed.  All material for a resource should be contained inside a single folder.  For easy access to resources, make sure to organize the information.  For example, if your resource is written about a shadowmapping in OpenGL, the resource belongs in the following folder structure: `OpenGL/ShadowMapping/ResourceNameHere`.  The initial page of the tutorial should be titled Index.md.  When possible, try to host images, videos, etc. on other platforms.

Topics may be duplicated under the following conditions:
* There are new implementations that solve the problem described.
* The implementation is written in a language sufficiently different from the original resource.

## Formatting Guide

All resources must adhere to the following style guide.  Any resource that do not adhere will be rejected until such a time that it adheres.

* Resource Name - Heading Level 1
* Section Name - Heading Level 2
* Subsection Name - Heading Level 3
* Code - Highlighted using Markdown using the language specifier
* Links to Internal Resources - Use the Markdown hyperlink style
* Links to External Resources - Use the raw URL

## Review of Resources

All resources created will be reviewed for correctness and adherence to this CONTRIBUTING guide before being committed into the repository by at least 1 SME.  Rejected or deferred resources will be given a reason in the pull request.
