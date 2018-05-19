open Types;

module CreateGameLoop: ((Types.Positions) => (Types.GameLoop)) = (Pos: Types.Positions) => {
  let loopCost = (1. /. Pos.divisor);
  let rec continue: game => game =
  game =>
    if (Pos.isActive(game.player.stats)) {
      game;
    } else {
      let updatedGame = Pos.incrementAll(game.level.map)
        |> map => { ... game, level: { ... game.level, map: map }}
        |> g => {... g, turn: g.turn +. loopCost } 
        |> g => {... g, player: {... g.player, stats: Pos.increment(g.player.stats)}};
        
      continue(updatedGame);
    };
};