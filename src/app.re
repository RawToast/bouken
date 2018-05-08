[%bs.raw {|require('./app.css')|}];

open Level;
open ReasonReact;
open GameMap;
open StaticData;

type actions =
  | MovePlayer(int, int);

let component = ReasonReact.reducerComponent("App");

let movement = (x, y) => MovePlayer(x, y);

let resultToUpdate = Rationale.Result.result(s => ReasonReact.Update(s), _f => NoUpdate);

let make = (_children) => {
  ...component,
  initialState: () => StaticData.initLevel,
  reducer: (act, level) =>
    switch act {
    | MovePlayer(x, y) => level |> Level.movePlayer(x, y) |> resultToUpdate;
  },
  render: (self) =>
    <div className="App">
      <GameMap level=(self.state) movePlayer=(x => self.reduce((y) => MovePlayer(x, y)))/>
    </div>
};
