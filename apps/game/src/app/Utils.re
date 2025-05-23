/* Empty function for CSS imports - handled by Vite */
let requireCSS = (_path: string) => ();

/* For Vite, we'll use import.meta.url to resolve asset paths */
let requireAssetURI = (path: string) => {
  /* In Vite, assets in public/ are served from the root */
  let cleanPath = path |> Js.String.replace("../../public/", "/");
  cleanPath;
};

/* Disable HMR for now */
let hot = false;

/* Empty accept function */
let accept = () => ();