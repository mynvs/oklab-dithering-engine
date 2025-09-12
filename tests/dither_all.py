from os import listdir, system

palette_list =[
	'2','8','12','18','27','36','48','64','100','150','252',#'512','2197',
	#'2_R0','2_T0','2_Y0','2_L0','2_G0','2_H0','2_C0','2_W0','2_B0','2_N0','2_M0','2_K0',
	'3_YB0','3_GM0', #'3_HK0','3_LN0','3_RC0','3_TW0',
	'4_CMY0','4_RGB0','4_NHT0','4_WKL0','4_RYB0','4_BY10',
	'5_YHBK0','5_RLCN0','5_TGWM0',
	#'7_RYGCBM0','7_TLHWNK0',
	'grey12','grey256',#'grey3','CMYx4',
	'rainbow1530',#'rainbow6','rainbow12',
	'gameboy','c64','nes','ega','cga','vga',
	'alphabet','mspaint',#'calculator',
	'seveirein32',#'seveirein128','seveirein256','urbmarl',
	#'minus17','frankie','iso_church','milk9','gheivel',
]

use_all_palettes = True

if use_all_palettes:
    system(f'../build/dither --allp')
else:
	for filename in palette_list:
		print(f'../build/dither -o output/{filename} -c {filename}')
		system(f'../build/dither -o output/{filename} -c {filename}')
