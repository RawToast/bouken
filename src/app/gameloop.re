open Types;
open Rationale;

module CreateGameLoop: ((Types.Positions) => (Types.GameLoop)) = (Pos: Types.Positions) => {
  open World;

  /* let enemyLogic = enemy => enemy; */
  type enemyInfo = {
    enemy: enemy,
    position: (int, int)
  };
  let loopCost = (1. /. Pos.divisor);
  let rec continue: game => game =
    game =>
      if (Pos.isActive(game.player.stats)) {
        game;
      } else {
        /* This code should be moved to another module */
        let findActiveEnemies = area => 
          area |> List.mapi((xi: int, xs: list(place)) => 
            xs |> List.mapi((yi: int, place: place) => switch place.state {
            | Enemy(e) => [ { enemy: e, position: (xi, yi) }]
            | _ => []
            } )
          ) |> List.flatten |> List.flatten;

        let activeEnemies: list(enemyInfo) = game.world 
          |> World.currentLevel
          |> Option.fmap(l => findActiveEnemies(l.map))
          |> Option.default([]);

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
