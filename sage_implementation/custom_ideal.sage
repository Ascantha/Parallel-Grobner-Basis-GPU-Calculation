execfile('f5_2.py');

R.<x,y,z,k> = QQ[]
I = R.ideal([4.2 + 3*x + x^2, 1 + y^2, 4 + z^3, 2*k*x]).homogenize()

print(R);
print('\n=======================================================================')
gb = f5_2(I, use_cpu = false, field_size = 65521); f5_2.print_stats();
print(gb);