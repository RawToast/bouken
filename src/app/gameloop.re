open Types;
open Rationale;

module CreateGameLoop = (Pos: Types.Positions, EL: EnemyLoop) => {
  open World;

  let loopCost = (1. /. Pos.divisor);
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
        Js.Console.log(activeEnemy.enemy.name ++ " is active");        

        let updatedGame = Option.bind(levelOpt, level =>
          EL.enemyLogic(activeEnemy, level, game)
        );
        Js.Console.log("is some " ++ string_of_bool(Option.isSome(updatedGame)));

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
