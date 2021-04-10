for i in `seq -w 1 9`
do
#   touch `date +%Y%m%d`-$i.txt
  filename="walkcolor000"$i".png"
  out_file="output"$i".png"
  echo $filename
  convert -flop $filename $out_file

done

for i in `seq -w 10 12`
do
#   touch `date +%Y%m%d`-$i.txt
  filename="walkcolor00"$i".png"
  out_file="output"$i".png"
  echo $filename
  convert -flop $filename $out_file

done