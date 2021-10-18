import os
import click

# Find all files in this directory
path = "./"
fname = []
for root,d_names,f_names in os.walk(path):
    for f in f_names:
        fname.append(os.path.join(root, f))

# Files that end with .cc
rename_list = []
for filename in fname:
    if filename.endswith('.cc'):
        rename_list.append(filename)
        print(filename)

if len(rename_list) > 0:
    if click.confirm('Do you want to continue?', default=False):
        print('Continuing')

        for filename in rename_list:
            prev_fn = filename
            new_fn = filename[:-3] + '.cpp'
            os.rename(prev_fn, new_fn)

        print('Done')
    else:
        print('Stopping')
else:
    print("No files to rename")



#
