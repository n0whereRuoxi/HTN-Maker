( define ( problem probname )
   ( :domain blocks4 )
   ( :requirements :strips :typing :equality )
   ( :objects
     b000 - block
     b001 - block
     b002 - block
     b003 - block
   )
   ( :init
     ( on-table b000 )
     ( on b001 b000 )
     ( on b002 b001 )
     ( on b003 b002 )
     ( clear b003 )
     ( hand-empty )
   )
   ( :goal
     ( and
       ( clear b000 )
     )
   )
 )
