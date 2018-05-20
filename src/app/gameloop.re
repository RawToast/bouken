open Types;

module CreateGameLoop: ((Types.Positions) => (Types.GameLoop)) = (Pos: Types.Positions) => {
  open World;

  let loopCost = (1. /. Pos.divisor);
  let rec continue: game => game =
    game =>
      if (Pos.isActive(game.player.stats)) {
        game;
      } else {
        let maybeGame = game.world 
          |> World.currentLevel
          |> Rationale.Option.fmap(l => {
            let area = Pos.incrementAll(l.map);
            let world = World.updateLevel( {...l, map: area }, game.world);
            { ...game, world: world, turn: game.turn +. loopCost }
            })
          |> Rationale.Option.fmap(g => {... g, player: {... g.player, stats: Pos.increment(g.player.stats)}});

        switch maybeGame {
        | None => game
        | Some(g) => continue(g)
        };
    };
};