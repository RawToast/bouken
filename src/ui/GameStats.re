// let text = 
[@react.component]
let make = (~player: Types.player, ~turn: float, ~level: string) => {
  <div className="GameStats">
    <text>
      {ReasonReact.string(
         player.name
         ++ " HP: "
         ++ string_of_int(player.stats.health)
         ++ " Gold: "
         ++ string_of_int(player.gold)
         ++ " Turn: "
         ++ Js.Float.toFixedWithPrecision(~digits=1, turn),
       )}
    </text>
    <div> <text> {ReasonReact.string(level)} </text> </div>
  </div>;
};
