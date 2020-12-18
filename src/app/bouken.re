module CreateGame:
  (Types.GameLoop, Types.World, Types.WorldBuilder) => Types.Game =
  (GL: Types.GameLoop, W: Types.World, WB: Types.WorldBuilder) => {
    module Area = Level.Area;
    open Types;
    module Option = Belt_Option;
    module Result = Belt_Result;

    let andUpdateTiles = game => {
      W.currentLevel(game.world)
      ->Option.map(lvl =>
          {
            ...lvl,
            map:
              Pathing.VisionUtil.updateTiles(
                ~limit=5,
                lvl.map,
                game.player.location,
              ),
          }
        )
      ->Option.map(lvl => W.updateLevel(lvl, game.world))
      ->Option.map(w => {...game, world: w})
      ->Option.getWithDefault(game);
    };

    let resultUpdateVision = res =>
      switch (res) {
      | Success(g) => Success(g |> andUpdateTiles)
      | EndGame(int, string) => EndGame(int, string)
      | Fail(string) => Fail(string)
      };

    let updateVision = res => res |> andUpdateTiles;

    let initPlayer = name => {
      name,
      stats: {
        health: 10,
        speed: 1.0,
        position: 0.,
        damage: 3,
      },
      gold: 0,
      location: (6, 6),
    };

    let initgame = pname =>
      initPlayer(pname) |> (p => {player: p, world: WB.create(p), turn: 0.});

    let create = name => initgame(name) |> andUpdateTiles;

    let calculateScore = game => {
      let baseScore = 1000;
      let turnPenalty = int_of_float(game.turn /. 5.);
      let goldBonus = game.player.gold * 10;
      let healthBonus = game.player.stats.health * 25;

      baseScore + healthBonus + goldBonus - turnPenalty;
    };

    let gameToRes = game =>
      if (game.player.stats.health > 0) {
        Success(game);
      } else {
        EndGame(calculateScore(game), game.player.name);
      };

    let attack = (x, y, game) => {
      let level = W.currentLevel(game.world);
      let player = game.player;
      let (xl, yl) = player.location;
      let tx = x + xl;
      let ty = y + yl;

      let findPlaceToAttack = l => Area.getPlace(tx, ty, l.map);

      let attackPlace = p =>
        switch (p.state) {
        | Enemy(e) =>
          let newEnemy = {
            ...e,
            stats: {
              ...e.stats,
              health: e.stats.health - player.stats.damage,
            },
          };
          if (newEnemy.stats.health > 0) {
            Some({...p, state: Enemy(newEnemy)});
          } else {
            Some({...p, state: Empty});
          };
        | _ => None
        };

      let updateLevelWithPlayer = level =>
        (level.map
        |> Area.setPlayerAt(xl, yl, player, 1.))
        -> Belt_Result.mapWithDefault(None, x => Some(x))
        -> Option.map(area => {...level, map: area});

      let goldBonus = (x, y, game) => {
        let player = game.player;

        let updateCurrentLevel = area =>
          W.currentLevel(game.world)
          -> Option.map(l => {...l, map: area})
          -> Option.map(l => W.updateLevel(l, game.world));

        let applyBonus = gam =>
          W.currentLevel(gam.world)
          -> Option.flatMap(
            level => {
              let bonus =
                Area.getPlace(x, y, level.map)
                -> Option.map(p =>
                     switch (p.state) {
                     | Empty => 5
                     | _ => 0
                     }
                   )
                -> Option.getWithDefault(0);

              let updatedPlayer = {...player, gold: player.gold + bonus};

              Area.setPlayerAt(xl, yl, updatedPlayer, 0., level.map)
              -> Result.mapWithDefault(None, x => Some(x))
              -> Option.flatMap(updateCurrentLevel)
              -> Option.map(w => {...gam, world: w, player: updatedPlayer});
            }
          );

        game |> applyBonus;
      };
      module G = Gameutil.GameUtil(Area, W);
      level
      -> Option.flatMap(findPlaceToAttack)
      -> Option.flatMap(attackPlace)
      -> Option.flatMap(p => level -> Option.map(Level.modifyTile(tx, ty, p)))
      -> Option.flatMap(updateLevelWithPlayer)
      -> Option.map(l => G.update(l.map, game))
      -> Option.flatMap(goldBonus(tx, ty))
      -> Option.map(GL.continue)
      -> Option.map(gameToRes)
      -> Option.getWithDefault(Fail("Unable to attack"));
    };

    let movePlayer = (x, y, game) => {
      let level = W.currentLevel(game.world);
      let (xl, yl) = game.player.location;
      let canMove =
        level
        -> Option.map(l =>
             Area.canMoveTo(
               ~overwrite=(x == 0 && y == 0),
               x + xl,
               y + yl,
               l.map,
             )
             -> Result.isOk
           )
        -> Option.getWithDefault(false);
      let canAttack =
        level
        -> Option.map(l =>
             Area.canMoveTo(~overwrite=true, x + xl, y + yl, l.map)
             -> Result.isOk
           )
        -> Option.getWithDefault(false);

      let attemptMove = (level, x, y) =>
        level
        -> Option.flatMap(_, l =>
             (l.map
             |> Area.movePlayer(x, y, 1.))
             -> Result.mapWithDefault(None, x=> Some(x))
             -> Option.map((pa: playerArea) => {
                  let nl = {...l, map: pa.area};
                  let world = W.updateLevel(nl, game.world);
                  {...game, player: pa.player, world};
                })
           )
        -> Option.map(GL.continue)
        -> Option.mapWithDefault(Fail("Unable to move player"), gameToRes);

      if (canMove) {
        attemptMove(level, x, y);
      } else if (canAttack) {
        attack(x, y, game);
      } else {
        Fail("Unable to move player");
      };
    };

    let useStairs = game => {
      let (cx, cy) = game.player.location;
      let currentLevel =
        W.currentLevel(game.world)
        -> Option.map(Level.removeOccupant(cx, cy));

      let currentLink: option(link) =
        currentLevel
        -> Option.flatMap(
             _,
             level => {
               let (x, y) = game.player.location;
               Area.getPlace(x, y, level.map)
               -> Option.flatMap(_, p =>
                    switch (p.tile) {
                    | STAIRS(link) => Some(link)
                    | _ => None
                    }
                  );
             },
           );

      let findLinkedLevel: link => option(level) =
        link => W.selectLevel(link.level, game.world);
      let stairs: (level, link) => option(place) =
        (lev, link) => Area.findStairs(link.id, lev.map);

      let findLinkLocation: option(link) => option(place) =
        optLink =>
          Option.flatMap(
            optLink,
            link => {
              let newLevel: option(level) = findLinkedLevel(link);
              let levelStairs: option(place) =
                Option.flatMap(newLevel, lev => stairs(lev, link));
              levelStairs;
            },
          );

      let coord: (option(level), option(place)) => option((int, int)) =
        (optL, optP) =>
          Option.flatMap(optL, level => {
            Option.flatMap(optP, place => {
              switch (place.tile) {
              | STAIRS(link) => Area.locationOfStairs(link.id, level.map)
              | _ => None
              }
            })
          });

      let linkedLevel = Option.flatMap(currentLink, findLinkedLevel);
      let linkedPlace = findLinkLocation(currentLink);

      let positionOpt = coord(linkedLevel, linkedPlace);

      Option.flatMap(currentLevel, prevLevel => {
        Option.flatMap(
          positionOpt,
          pos => {
            let (x, y) = pos;
            linkedLevel
            |> Option.flatMap(
                 _,
                 l => {
                   let world = game.world;
                   let current = l.name;
                   let pre = {
                     ...game,
                     player: {
                       ...game.player,
                       location: (x, y),
                       stats: {
                         ...game.player.stats,
                         position: game.player.stats.position -. 1.5,
                       },
                     },
                     world: {
                       levels: world.levels,
                       current,
                     },
                   };

                   let upAreaOpt =
                     Area.setPlayerAt(x, y, pre.player, 1., l.map)
                     -> Belt_Result.mapWithDefault(None, x => Some(x))
                   let upArea = upAreaOpt -> Option.getWithDefault(l.map);
                   let updatedLevel = {...l, map: upArea};

                   let nextWorld =
                     pre.world
                     |> W.updateLevel(prevLevel)
                     |> W.updateLevel(updatedLevel);

                   Some({...pre, world: nextWorld});
                 },
               );
          },
        )
      })
      -> Option.map(GL.continue)
      -> Option.mapWithDefault(Fail("Unable to use stairs"), x => Success(x));
    };

    let useExit = game => {
      let currentLevel = W.currentLevel(game.world);

      let currentExitScore: option(int) =
        currentLevel
        |> Option.flatMap(
             _,
             level => {
               let (x, y) = game.player.location;
               Area.getPlace(x, y, level.map)
               |> Option.flatMap(_, p =>
                    switch (p.tile) {
                    | EXIT(score) => Some(score)
                    | _ => None
                    }
                  );
             },
           );

      let baseBonus = 1000;

      switch (currentExitScore) {
      | Some(score) =>
        EndGame(score + baseBonus + calculateScore(game), game.player.name)
      | None => Success(game)
      };
    };
  };
