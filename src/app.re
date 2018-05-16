[%bs.raw {|require('./app.css')|}];

open ReasonReact;
open Bouken;
open Types;

type actions =
  | MovePlayer(int, int);

let component = ReasonReact.reducerComponent("App");

let movement = (x, y) => MovePlayer(x, y);

let resultToUpdate = r => switch r {
  | Some(s) => ReasonReact.Update(s)
  | None => NoUpdate
};

let make = (_children) => {
  ...component,
  initialState: () => Bouken.create("test"),
  reducer: (act, game) =>
    switch act {
    | MovePlayer(x, y) => game |> Bouken.movePlayer(x, y) |> resultToUpdate;
  },
  render: (self) =>
    <div className="App">
      <GameMap level=(self.state.level) movePlayer=(x => self.reduce((y) => MovePlayer(x, y)))/>
    </div>
};
