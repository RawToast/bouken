
open ReasonReact;

let component = ReasonReact.reducerComponent("StartView");

let make = (~startGame, _children) => {
  ...component,
  initialState: () => "",
  reducer: (state: string, _ext) =>  ReasonReact.Update(state),
  render: (self) => 
    <div>
      <h3>(stringToElement("Bouken"))</h3>
      <input onChange=(evt => self.reduce(() => ReactDOMRe.domElementToObj(ReactEventRe.Form.target(evt))##value)())></input>
      <div><button onClick=(_ => startGame(self.state))>(stringToElement("Start"))</button></div>
    </div>
};
