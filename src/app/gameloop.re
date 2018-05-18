open Types;

module CreateTurnLoop: ((Types.Positions) => (Types.TurnLoop)) = (Pos: Types.Positions) => {
    let rec continue: game => game =
    game =>
      if (Pos.isActive(game.player.stats)) {
        game;
      } else {
          Js.Console.log("Player not active");
        let updatedGame = Pos.incrementAll(game.level.map)
          |> map => { ... game, level: { ... game.level, map: map }}
          |> g => {... g, turn: g.turn +. 1.} 
          |> g => {... g, player: {... g.player, stats: Pos.increment(g.player.stats)}};
          
        continue(updatedGame);
      };
  };