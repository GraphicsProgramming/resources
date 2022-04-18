/**
 * Taken from https://www.w3schools.com/howto/howto_html_include.asp
 */
async function includeHTML() {
	var z, i, elmnt, file;
	/* Loop through a collection of all HTML elements: */
	z = document.getElementsByTagName("*");
	for (i = 0; i < z.length; i++) {
		elmnt = z[i];
		/*search for elements with a certain atrribute:*/
		file = elmnt.getAttribute("w3-include-html");
		if (file) {
			const response = await fetch(file).then(x => {
				if (x.status == 200) { x.text().then(text => elmnt.innerHTML = text); }
				else if (x.status == 404) { elmnt.innerHTML = "Page not found."; }
				/* Remove the attribute, and call this function once more: */
				elmnt.removeAttribute("w3-include-html");

				return includeHTML();
			});

			return Promise.all([ response ]);
		}
	}
}