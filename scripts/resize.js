function resizeBestOfTheYear() {
    var monthPreviews = document.getElementsByClassName("showcase-preview-element-img");
    var maxHeight = 0.0;
    Array.from(monthPreviews).forEach((preview) => {
		maxHeight = preview.clientHeight > maxHeight ? preview.clientHeight : maxHeight;
    });

	var yearPreviews = document.getElementsByClassName("showcase-preview-year-img");
	Array.from(yearPreviews).forEach((preview) =>  {
		if (maxHeight > 0.0) {
			preview.style.height = maxHeight * 2.0 + "px";
		}
	});
}
