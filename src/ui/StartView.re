
open ReasonReact;
open Types;

let component = ReasonReact.reducerComponent("StartView");

let make = (~startGame, _children) => {
  ...component,
  initialState: () => "Test",
  reducer: (state: string, _ext) =>  ReasonReact.Update(state),
  render: (self) => 
    <div>
      <input onChange=(evt => self.reduce(() => ReactDOMRe.domElementToObj(ReactEventRe.Form.target(evt))##value)())></input>
      <button onClick=(_ => startGame(self.state))>(stringToElement("Start Game"))</button>
    </div>
};
