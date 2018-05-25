
open ReasonReact;open Types;


let component = ReasonReact.reducerComponent("StartView");

let make = (_children) => {
  ...component,
  initialState: () => "",
  reducer: (state: string, _ext) =>  ReasonReact.Update(state),
  render: (_) => 
    <div>
      <button label="Hey" />
      
    </div>
};
