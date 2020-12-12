open Types;
open Rationale;

module CreateGameLoop = (Pos: Types.Positions, EL: EnemyLoop) => {
  let loopCost = (1. /. Pos.divisor);
  module World = World.World;
  
  let rec continue: game => game =
    game => {
      let activeEnemies: list(enemyInfo) = game.world 
          |> World.currentLevel
          |> Option.fmap(l => EL.findActiveEnemies(l.map))
          |> Option.default([]);

      if (Pos.isActive(game.player.stats)) {
        game;
      } else if (List.length(activeEnemies) >= 1) {
        /* relocate */
        let levelOpt = World.currentLevel(game.world);
        let activeEnemy = List.hd(activeEnemies);
        let updatedGame = Option.bind(levelOpt, level =>
          EL.takeTurn(activeEnemy, level, game)
        );

        switch updatedGame {
          | None => game
          | Some(g) => continue(g)
          };
      } else {
        /* No one is active */
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
};
