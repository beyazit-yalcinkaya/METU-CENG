main_input=raw_input(">>> ")
main_input_list=list(main_input)

while len(main_input)==8 and main_input_list[6]=="-":

    digit_1, digit_2, digit_3, digit_4, digit_5, digit_6, digit_7, digit_8 = main_input_list[0], main_input_list[1], main_input_list[2], main_input_list[3], main_input_list[4], main_input_list[5], main_input_list[6], main_input_list[7]
    main_input_only_num, main_input_num_wout_1st, main_input_num_wout_2nd, main_input_num_wout_3rd, main_input_num_wout_4th, main_input_num_wout_5th, main_input_num_wout_6th, main_input_num_wout_8th=[digit_1,digit_2,digit_3,digit_4,digit_5,digit_6,digit_8], [digit_2,digit_3,digit_4,digit_5,digit_6,digit_8], [digit_1,digit_3,digit_4,digit_5,digit_6,digit_8], [digit_1,digit_2,digit_4,digit_5,digit_6,digit_8], [digit_1,digit_2,digit_3,digit_5,digit_6,digit_8], [digit_1,digit_2,digit_3,digit_4,digit_6,digit_8], [digit_1,digit_2,digit_3,digit_4,digit_5,digit_8], [digit_1,digit_2,digit_3,digit_4,digit_5,digit_6]

    while all(i in "0123456789?" for i in main_input_only_num):

        def odd_num_f(x,y,z,t):

            if 2*int(digit_2)>=10:
                _2xdigit_2=int(list(str(2*int(digit_2)))[0])+int(list(str(2*int(digit_2)))[1])
            else:
                _2xdigit_2=2*int(digit_2)
            if 2*int(digit_4)>=10:
                _2xdigit_4=int(list(str(2*int(digit_4)))[0])+int(list(str(2*int(digit_4)))[1])
            else:
                _2xdigit_4=2*int(digit_4)
            if 2*int(digit_6)>=10:
                _2xdigit_6=int(list(str(2*int(digit_6)))[0])+int(list(str(2*int(digit_6)))[1])
            else:
                _2xdigit_6=2*int(digit_6)

            total_sum=_2xdigit_2+int(y)+_2xdigit_4+int(z)+_2xdigit_6+int(t)
            
            if total_sum%10==0:
                x="0"
            elif total_sum>10 and total_sum%10!=0:
                x=str(10-int(list(str(total_sum))[1]))
            else:
                x=str(10-total_sum)
            return x

        def even_num_f(x,y,z):

            if 2*int(y)>=10:
                _2xdigit_4=int(list(str(2*int(y)))[0])+int(list(str(2*int(y)))[1])
            else:
                _2xdigit_4=2*int(y)
            if 2*int(z)>=10:
                _2xdigit_6=int(list(str(2*int(z)))[0])+int(list(str(2*int(z)))[1])
            else:
                _2xdigit_6=2*int(z)

            total_sum=int(digit_1)+int(digit_3)+_2xdigit_4+int(digit_5)+_2xdigit_6+int(digit_8)

            if total_sum%2==0 and total_sum%10==0:
                x="0"
            elif total_sum%2==0 and total_sum>10 and total_sum%10!=0:
                x=str((10-int(list(str(total_sum))[1]))/2)
            elif total_sum%2==0 and total_sum<10:
                x=str((10-total_sum)/2)
            if total_sum%2==1 and total_sum>10:
                x=str(((10-int(list(str(total_sum))[1]))+9)/2)
            elif total_sum%2==1 and total_sum<10:
                x=str(((10-total_sum)+9)/2) 
            return x

        def check_f():

            if 2*int(digit_2)>=10:
                _2xdigit_2=int(list(str(2*int(digit_2)))[0])+int(list(str(2*int(digit_2)))[1])
            else: 
                _2xdigit_2=2*int(digit_2)
            if 2*int(digit_4)>=10:
                _2xdigit_4=int(list(str(2*int(digit_4)))[0])+int(list(str(2*int(digit_4)))[1])
            else:
                _2xdigit_4=2*int(digit_4)
            if 2*int(digit_6)>=10:
                _2xdigit_6=int(list(str(2*int(digit_6)))[0])+int(list(str(2*int(digit_6)))[1])
            else:
                _2xdigit_6=2*int(digit_6)

            total_sum=int(digit_1)+_2xdigit_2+int(digit_3)+_2xdigit_4+int(digit_5)+_2xdigit_6

            if total_sum%10==0:
                valid_digit="0"
            elif total_sum>10 and total_sum%10!=0:
                valid_digit=str(10-int(list(str(total_sum))[1]))
            else:
                valid_digit=str(10-total_sum)
            if valid_digit==digit_8:
                print "VALID"
            else:
                print "INVALID"

        if digit_1=="?" and all(a in "0123456789" for a in main_input_num_wout_1st):
            print odd_num_f(digit_1, digit_3, digit_5, digit_8)+digit_2+digit_3+digit_4+digit_5+digit_6+digit_7+digit_8
            break
        elif digit_2=="?" and all(b in "0123456789" for b in main_input_num_wout_2nd):
            print digit_1+even_num_f(digit_2, digit_4, digit_6)+digit_3+digit_4+digit_5+digit_6+digit_7+digit_8
            break    
        elif digit_3=="?" and all(c in "0123456789" for c in main_input_num_wout_3rd):
            print digit_1+digit_2+odd_num_f(digit_3, digit_1, digit_5, digit_8)+digit_4+digit_5+digit_6+digit_7+digit_8
            break      
        elif digit_4=="?" and all(d in "0123456789" for d in main_input_num_wout_4th):
            print digit_1+digit_2+digit_3+even_num_f(digit_4, digit_2, digit_6)+digit_5+digit_6+digit_7+digit_8
            break      
        elif digit_5=="?" and all(e in "0123456789" for e in main_input_num_wout_5th):
            print digit_1+digit_2+digit_3+digit_4+odd_num_f(digit_5, digit_1, digit_3, digit_8)+digit_6+digit_7+digit_8
            break       
        elif digit_6=="?" and all(f in "0123456789" for f in main_input_num_wout_6th):
            print digit_1+digit_2+digit_3+digit_4+digit_5+even_num_f(digit_6, digit_2, digit_4)+digit_7+digit_8
            break    
        elif digit_8=="?" and all(g in "0123456789" for g in main_input_num_wout_8th):
            print digit_1+digit_2+digit_3+digit_4+digit_5+digit_6+digit_7+odd_num_f(digit_8, digit_1, digit_3, digit_5)
            break                   
        elif all(h in "0123456789" for h in main_input_only_num):
            check_f()
            break    
        else:
            print "INVALID"
            break

        break

    else:
        print "INVALID"
        break
    break
else:  print "INVALID"
