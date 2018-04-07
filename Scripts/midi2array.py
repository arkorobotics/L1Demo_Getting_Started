import midi
import sys
notenames=['C','Db','D','Eb','E','F','Gb','G','Ab','A','Bb','B']
file = midi.read_midifile(sys.argv[1])
tracks=[]
for track in file:
  if len(track)>2:
    trk=[]
    for event in track:
      if type(event) == midi.events.NoteOnEvent:
        trk.append(event)
    tracks.append(trk)
trx=[]
for track in tracks:
  if len(track)>2:
    trx.append(track)
tracks=trx
channels=[]
fx=[]
res=file.resolution
for track in tracks:
  chan=[]
  cfx=[]
  lastnote=('D0',0)
  for note in track:
    while note.tick > 0:
       chan.append(lastnote)
       if note.tick > res:
       	 cfx.append(0)
       else:
         cfx.append(1)
       note.tick -= res
    lastnote=(notenames[note.pitch%12]+str(note.pitch/12),note.velocity)
  channels.append(chan)
  fx.append(cfx)
oldfx=fx
fx=[]
for i in range(len(oldfx[0])):
  n=0
  s=0
  for chan in oldfx:
    try:
      if chan[i]:
        s+=4**n
    except:
      pass
    n+=1
  fx.append(str(s))
output = open('song.h','w+')
output.write('/* auto-generated from midi*/\n')
i=1
for chan in channels:
  output.write('__prog__ unsigned short c'+str(i)+'f[] __attribute__((space(prog))) = {\n')
  for note in chan:
    output.write(note[0]+',')
  output.write('\n};\n')
  output.write('__prog__ unsigned short c'+str(i)+'a[] __attribute__((space(prog))) = {\n')
  for note in chan:
    output.write(str(note[1])+',')
  output.write('\n};\n')
  i+=1
output.write('__prog__ unsigned short fltr[] __attribute__((space(prog))) = {\n')
output.write(','.join(fx))
output.write('\n};\n')

