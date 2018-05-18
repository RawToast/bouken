open Types;

let component = ReasonReact.statelessComponent("GameStats");

let make = (~player: player, ~turn: float, _children) => {
  ...component,
  render: _self =>
    <div className="GameStats">
      <text>(ReasonReact.stringToElement("Turn: " ++ string_of_float(turn)))</text>
    </div>,
}