[@react.component]
let make = (~startGame, ~onChange) => {
  let (name, setName) = React.useState(() => "");

  let setValue = e => {
    let value = ReactEvent.Synthetic.target(e)##value;
    setName(_ => value);
    onChange(value);
  };

  <div>
    <h1> {React.string("Bouken")} </h1>
    <input
      className="nameinput"
      value=name
      onChange=setValue
      onKeyPress=startGame
    />
    <br />
    <Instructions />
    <Key></Key>
    <PatchNotes></PatchNotes>
  </div>;
};
