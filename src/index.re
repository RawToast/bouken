[%bs.raw {|require('./index.css')|}];

[@bs.module "./registerServiceWorker"]
external register_service_worker: unit => unit = "default";

ReactDOMRe.renderToElementWithId(<App />, "root");
ReasonReact.Router.push("");

register_service_worker();
