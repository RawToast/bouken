[@react.component]
let make = (~startGame) => {
  let (name, setName) = React.useState(() => "");

  {
    <div>
      <h1>(React.string("Bouken"))</h1>
      <input className="nameinput"
        value=name
        onChange={event => (_ => ReactEvent.Form.target(event)##value) |> setName}
        onKeyPress=(evt => if (ReactEvent.Keyboard.key(evt) == "Enter") { 
          startGame(name)
        })></input><br/>
        // <Instructions></Instructions>
        // <Key></Key>
        // <PatchNotes></PatchNotes>
    </div>
  }
};
