Name: Aditya Manjrekar
email id: amanjre1@binghamton.edu

Name: Suhas Mastud
email id: smastud1@binghamton.edu

Steps to Execute:
1) make clean
2) make
3) sudo insmod mypipe.ko
4) sudo chmod a+r+w /dev/mypipe
5) gcc producer.c -o producer
6) gcc consumer.c -o consumer
7) ./producer
8) ./consumer
