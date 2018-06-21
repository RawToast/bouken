open ReasonReact;

let component = ReasonReact.reducerComponent("StartView");

let make = (~startGame, _children) => {
  ...component,
  initialState: () => "",
  reducer: (state: string, _ext) =>  ReasonReact.Update(state),
  render: (self) => {
    <div>
      <h2>(string("Bouken"))</h2>
      <input className="nameinput"
        onChange=(evt => self.send(ReactDOMRe.domElementToObj(ReactEventRe.Form.target(evt))##value))
        onKeyPress=(evt => if (ReactEventRe.Keyboard.key(evt) == "Enter") { 
          startGame(self.state)
        })></input><br/>
        <Instructions></Instructions>
        <Key></Key>
        <PatchNotes></PatchNotes>
    </div>
  }
};
