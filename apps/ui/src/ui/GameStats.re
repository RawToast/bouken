// let text = 
[@react.component]
let make = (~player: BoukenGame.Types.player, ~turn: float, ~level: string) => {
  <div className="GameStats">
    <p>
      {ReasonReact.string(
         player.name
         ++ " HP: "
         ++ string_of_int(player.stats.health)
         ++ " Gold: "
         ++ string_of_int(player.gold)
         ++ " Turn: "
         ++ Js.Float.toFixedWithPrecision(~digits=1, turn),
       )}
    </p>
    <div> <p> {ReasonReact.string(level)} </p> </div>
  </div>;
};
