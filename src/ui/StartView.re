open ReasonReact;

let component = ReasonReact.reducerComponent("StartView");

let make = (~startGame, _children) => {
  ...component,
  initialState: () => "",
  reducer: (state: string, _ext) =>  ReasonReact.Update(state),
  render: (self) => {
    <div>
      <h3>(string("Bouken"))</h3>
      <input 
        onChange=(evt => self.send(ReactDOMRe.domElementToObj(ReactEventRe.Form.target(evt))##value))
        onKeyPress=(evt => if (ReactEventRe.Keyboard.key(evt) == "Enter") { 
          startGame(self.state)
        })></input>
    </div>
  }
};
