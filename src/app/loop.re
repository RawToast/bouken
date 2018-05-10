open Types;

module Loop = {
  let inc = stats => {...stats, position: stats.position +. stats.speed};
  
  let incrementPositions =
    List.map((ys: list(place)) =>
      ys
      |> List.map((place: place) =>
           switch (place.state) {
           | EMPTY => place
           | PLAYER(p) => {
               ...place,
               state: PLAYER({...p, stats: inc(p.stats)}),
             }
           | ENEMY(e) => {
               ...place,
               state: ENEMY({...e, stats: inc(e.stats)}),
             }
           }
         )
    );
  let update = area => area |> incrementPositions;
};