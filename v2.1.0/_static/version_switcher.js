document.addEventListener("DOMContentLoaded", function() {
    // Define the root of your GitHub Pages site
    const siteRoot = "/licensecc"; 
    const jsonUrl = `${siteRoot}/versions.json`;

    fetch(jsonUrl)
        .then((response) => response.json())
        .then((versions) => {
            let select = document.createElement("select");
            select.id = "version-switcher";
            select.style.margin = "10px";
            select.style.padding = "5px";
            select.style.width = "90%";
            select.style.backgroundColor = "#fcfcfc";
            select.style.border = "1px solid #ccc";
            
            const currentPath = window.location.pathname;
            
            versions.forEach((v) => {
                let option = document.createElement("option");
                option.value = v.url;
                option.text = v.version;
                // Auto-select the version we are currently viewing
                if (currentPath.includes(v.url)) {
                    option.selected = true;
                }
                select.appendChild(option);
            });

            select.addEventListener("change", function() {
                window.location.href = this.value;
            });

            // Target the Sphinx RTD Theme search box to inject the dropdown
            // If you use a different theme, change this CSS selector
            let searchBox = document.querySelector(".wy-side-nav-search");
            let title = document.querySelector(".icon-home");
            if (searchBox) {
                searchBox.insertBefore(select,title.nextSibling);
            }
        })
        .catch((error) => console.error("Error loading versions.json:", error));
});