# Using LaTex in markdown

Since GitHub markdown doesn't natively support LaTex, we use markdeep.  
Specifically, a slightly modified version of markdeep to look like GitHub markdown.

Using markdeep is relatively straight forward. You write your markdown as you normally would, and use the special markdeep syntax for its extra features (the syntax is explained on [their website](https://casual-effects.com/markdeep/)).  
Besides that you add the script snippets at the bottom of your markdown file, and save it with the extension `.md.html`.  
The script snippets can be found in [template.md.html](template.md.html).

# Requirements

1. You are not allowed to use a different style sheet, we want all pages to look uniform across the website.
2. In the script tags use **relative** paths to the style sheet and script.  
This is so that the markdown can be viewed in the browser without a webserver. Which also makes it easier to write as you can test it locally.
3. When linking to normal markdown, use paths ending in `filename.html` and not `filename.md`, this is to make the browsing experience on GitHub pages leaner. When the path ends with `index.md` just supply the folder path.  
As far as I know there's no way to link to both as with normal GitHub markdown, where we can just point to the .md file and have it be transformed to the html path in GitHub pages. So we opt for linking to the generated HTML versions here.
