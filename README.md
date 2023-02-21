# stl-tweaker

### points []

Όλα τα _διαφορετικά_ σημεία P(x, y, z) σε ένα πίνακα `points`:

[x1, y1, z1], [x2, y2, z2] ...

Για να απευθυνθεί κανείς σε ένα σημειο το κάνει χρησιμοποιώντας ένα index, `points_i` σε αυτόν τον πίνακα.

### faces

Το face αντιστοιχεί σε ένα τρίγωνο. Κάθε face έχει τρία σημεία. Τα σημεία αυτά ορίζονται από τρεις δείκτες σαν το `points_i`:

face.a, face.b, face.c

Ο πίνακας faces περιέχει όλα τα διαφορετικά faces. Με τυχαία σειρά.

### graph

Ο πίνακας graph έχει όλες τις διασυνδέσεις μεταξύ των σημείων. Θα θέλαμε, ξεκινώντας από ένα τυχαίο σημείο, να μπορούμε να πάμε με μία κίνηση σε όλα όσα συνδέονται με αυτό.

π.χ. το σημείο 5 (δηλ. points[5]) συνδέεται με τα 3,4,7 κ.λπ. 

Κάθε σημείο δηλαδή πρέπει να κρατά μια λίστα με τα υπόλοιπα σημεία (indices στο points[]) με τα οποία συνδέεται. 

Για να φτιάξουμε αυτή τη δομή, θα μπορουσαμε να σαρώσουμε τα faces ένα ένα και για κάθε και κάθε ζευγάρι σημείων που βρισκουμε να καταχωρείται στις δύο λίστες των σημείων που το αποτελούν.


### Applications



### Implementation


### Tips

#### VAO, VBO and state

VAOs keep state about VBOs among others. The point when this state is stored is when _glVertexAttribPointer()_ is called. Thus, if you want to unbind the VBO before unbinding the VAO, you won't break anything. That is, the VBO will _still_ be related to the VAO.



