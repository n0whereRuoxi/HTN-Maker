( define ( htn-problem probname )
  ( :domain blocks4 )
  ( :requirements :strips :htn :typing :equality )
  ( :objects
    b001 - block
    b003 - block
    b002 - block
    b024 - block
    b008 - block
    b020 - block
    b017 - block
    b015 - block
    b012 - block
    b019 - block
    b013 - block
    b021 - block
    b007 - block
    b000 - block
    b011 - block
    b005 - block
    b018 - block
    b006 - block
    b010 - block
    b004 - block
    b009 - block
    b016 - block
    b022 - block
    b014 - block
    b023 - block
  )

  ( :init
    ( on-table b001 )
    ( on b003 b001 )
    ( on b002 b003 )
    ( on b024 b002 )
    ( on b008 b024 )
    ( on b020 b008 )
    ( clear b020 )
    ( on-table b017 )
    ( on b015 b017 )
    ( on b012 b015 )
    ( on b019 b012 )
    ( on b013 b019 )
    ( on b021 b013 )
    ( clear b021 )
    ( on-table b007 )
    ( on b000 b007 )
    ( on b011 b000 )
    ( on b005 b011 )
    ( on b018 b005 )
    ( on b006 b018 )
    ( on b010 b006 )
    ( on b004 b010 )
    ( on b009 b004 )
    ( on b016 b009 )
    ( clear b016 )
    ( on-table b022 )
    ( on b014 b022 )
    ( on b023 b014 )
    ( clear b023 )
    ( hand-empty )
  )
  ( :tasks
    ( make-on-table b020 )
    ( make-on b006 b020 )
    ( make-on b008 b006 )
    ( make-on b003 b008 )
    ( make-on b007 b003 )
    ( make-on b019 b007 )
    ( make-on-table b013 )
    ( make-on b012 b013 )
    ( make-on-table b016 )
    ( make-on b011 b016 )
    ( make-on b004 b011 )
    ( make-on b005 b004 )
    ( make-on-table b017 )
    ( make-on b010 b017 )
    ( make-on-table b002 )
    ( make-on b009 b002 )
    ( make-on b021 b009 )
    ( make-on b014 b021 )
    ( make-on-table b023 )
    ( make-on b022 b023 )
    ( make-on b001 b022 )
    ( make-on b024 b001 )
    ( make-on b000 b024 )
    ( make-on-table b015 )
    ( make-on b018 b015 )
  )
)

