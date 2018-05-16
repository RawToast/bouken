open Types;

module Loop: Positions = {
  let inc = stats => {...stats, position: stats.position +. stats.speed};
  let isActive = stats => stats.position >= 1.;
  let incrementAll =
    List.map((ys: list(place)) =>
      ys
      |> List.map((place: place) =>
           switch (place.state) {
           | EMPTY => place
           | PLAYER(p) => {...place, state: PLAYER({...p, stats: inc(p.stats)})}
           | ENEMY(e) => {...place, state: ENEMY({...e, stats: inc(e.stats)})}
           }
         )
    );
};
