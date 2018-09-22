open Utils;

requireCSS("./index.css");

/* [%bs.raw {|require('../public/world/*.csv')|}]; */

ReactDOMRe.renderToElementWithId(<App />, "index");
ReasonReact.Router.push("");

if (hot) {
  accept();
};
