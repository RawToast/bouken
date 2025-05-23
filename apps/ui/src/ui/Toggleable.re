[@react.component]
let make = (~label, ~content) => {
  let (showKey, updateState) = React.useState(() => false);
  let toggleKey = _ => updateState(show => !show);

  <div>
    <h3 onClick={toggleKey}> {React.string(label)} </h3>
    {showKey? content : ReasonReact.null}
  </div>;
};

