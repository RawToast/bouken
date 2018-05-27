open Types;

let component = ReasonReact.statelessComponent("GameStats");

let make = (~player: player, ~turn: float, ~level:string, _children) => {
  ...component,
  render: _self =>
    <div className="GameStats">
      <text>(ReasonReact.string(level ++ " Turn: " ++ string_of_float(turn)))</text>
    </div>,
}
