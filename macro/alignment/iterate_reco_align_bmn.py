# -----------------------------------------------------------------------------
# Script for finding global alignment corrections iteratively.
#
# Reconstruct limited number of events -> run alignment -> run new
# reconstruction of the same events using corrections obtained at the previous
# iteration and run the alignment again, and so on, until either corrections
# become small or the maximal number of iterations is reached. Namely, if
# maxNumOfIterations==1, then the subset reconstruction run and alignment are
# run only once i.e. iterativity is degenerate. The obtained corrections then
# simply used for the full reconstruction run. But starting from
# maxNumOfIterations==2 the subset reconstruction run and alignment may be run
# up to maxNumOfIterations times.
#
# After each iteration, starting from the second, the newly obtained
# corrections are added to the existing cumulative corrections that will
# eventually be used for the full reconstruction run.
#
# Now, when to stop? There are several possibilities:
#
# 1. a trivial one is to stop at maxNumOfIterations which does not make much
# sense, of course;
#
# 2. relatively cheap, but evidently not perfect, is to stop when the newly
# obtained corrections are <= their errors;
#
# 3. when track reconstruction precision reaches its plateau at a given set of
# all other conditions; this is perfect, but not so easy to formulate and
# implement; note, that this approach will generally stop earlier!
#
# digiFileName - input file with digis, like file_digi.root. To process
# experimental data, you can use 'runN-NNN:' prefix, e.g.
# "run5-458:../digits_run5/bmn_run0458_digi.root"
# then the geometry is obtained from the Unified Database.
#
#
## Not implemented yet in run_reco_bmn.C:
##
## However, if the file name is like
## "bmn_run06_Glob_digi_001203.root", then nothing to do: the geometry will be
## obtained from the Unified Database automatically without any prefix, although
## if prefix is still used, it also works correctly.
#
# nStartEvent - number (start with zero) of first event to process, default: 0
# nEvents - number of events to process
#
# alignCorrFileName - argument for choosing input file with the alignment
# corrections.
#
# If alignCorrFileName == 'default', (case insensitive) then corrections are
# retrieved from UniDb according to the running period and run number.
#
# If alignCorrFileName == '', then no corrections are applied at all.
#
# If alignCorrFileName == '<path>/<file-name>', then the corrections are taken
# from that file.
#
# Anatoly.Solomin@jinr.ru 2017-04-05
# -----------------------------------------------------------------------------
#!/usr/bin/python
import   os
import   re
import   sys
from     optparse   import OptionParser
from     distutils  import dir_util
from     os.path    import expandvars
from     subprocess import *
from     time       import sleep
from     time       import time
from     time       import gmtime, strftime
import __builtin__

def main() :
    usage = "usage: %prog [options] arg"
    parser = OptionParser(usage)
    parser.add_option("-v", "--verbose", dest="verbose",                default=True,                               help="print this to stdout")
    parser.add_option("-l", "--list",    dest="digiFileListFileName",   default='bmn_run06_Glob_digi_filelist.txt', help="take list of digi files from file")
    parser.add_option("-m", "--maxit",   dest="maxNumOfIterations",     default=1,                                  help="maximum number of iterations",          type="int")
    parser.add_option("-n", "--nev",     dest="nEvents",                default=10000,                              help="number of events to process",           type="int")
    parser.add_option("-i", "--inf",     dest="addInfo",                default='',                                 help="additional meta-information")
    parser.add_option("-c", "--corr",    dest="startAlignCorrFileName", default='default',                          help="file name with the starting alignment corrections")
    (options, args) = parser.parse_args()
    if options.verbose :
        print "reading %s..." % options.digiFileListFileName
    # Here we start iterations, each consisting of these steps:
    #
    # 1. reconstruction, either using only geometry from the database
    # (optionally updated using default or special set of corrections), or
    # (starting from the 2-nd iteration) additionally updateing it with the
    # cumulative misalignment corrections updated after each alignment iteration;
    #
    # 2. obtaining the misalignment values;
    #
    # 3. updating the cumulative misalignment values;
    #
    # 4. checking whether this is the last iteration, according to a certain
    # criterion (TBD).
    #
    # maxNumOfIterations is the brutal-force limit.

    # First store the list of digi fileas in vector digiFileNames,
    # so that the file with their list is read only once.
    #
    # Open file with list of input digi files chosen for running the alignment
    # and store them line-by-line in a list:
    print 'options.digiFileListFileName   = '+options.digiFileListFileName
    digiFileNames = [line.rstrip('\n') for line in open(options.digiFileListFileName)]

    # Define name of the file that contains up-to-date cumulative misalignment
    # compensations i.e. initial one (e.g. taken from UniDB) and updated with
    # new compensation corrections resulting from all preceeding iterations of
    # this alignment session.
    #
    # At the first iteration (we start from 1), the reconstruction does not use
    # any newly obtained corrections, as they do not exist yet.
    # It can use the fefault ones, or some special ones, or not use any.
    if options.startAlignCorrFileName != '' :
        # In this case the reconstruction will use concrete initial set of
        # corrections, e.g. current default corrections, when
        # options.startAlignCorrFileName == '$VMCWORKDIR/input/alignCorrsLocal_GEM.root'
        #
        # For the machinery to work correctly, we copy the initial corrections
        # to an appropriately named file marked as '_it00'.
        # First form the name:
        if options.addInfo != '' :
            sumAlignCorrFileName = options.digiFileListFileName.replace('digi_filelist.txt', options.addInfo+'_sum_align_it00.root')
        else :
            sumAlignCorrFileName = options.digiFileListFileName.replace('digi_filelist.txt',                  'sum_align_it00.root')
        # and now create the file with that name:
        print 'options.startAlignCorrFileName = '+options.startAlignCorrFileName
       #os.system('cp '+options.startAlignCorrFileName+' '+sumAlignCorrFileName)
        call([    'cp', options.startAlignCorrFileName,    sumAlignCorrFileName])
    else :
        # or we can also set it empty, just keep default
        # options.startAlignCorrFileName == '', if we want to run the alignment
        # form scratch;
        #
        # in this case a regular sumAlignCorrFileName file will be created only
        # at the end of the 1-st iteration;
        #
        # as to the reconstruction, for the BmnGemStripHitMaker, '' means that
        # no alignment corrections are to be used this time:
        sumAlignCorrFileName = ''
    preAlignCorrFileName = '' # just for consistency
    newAlignCorrFileName = '' # just for consistency
    print 'preAlignCorrFileName           = '+preAlignCorrFileName
    print 'newAlignCorrFileName           = '+newAlignCorrFileName
    print 'sumAlignCorrFileName           = '+sumAlignCorrFileName

    # file for storing names of files with new corrections
    if options.addInfo != '' :
        newAlignCorrFileListFileName = options.digiFileListFileName.replace('digi', options.addInfo+'_new_align')
    else :
        newAlignCorrFileListFileName = options.digiFileListFileName.replace('digi',                  'new_align')
    print 'newAlignCorrFileListFileName   = '+newAlignCorrFileListFileName
    # file for storing names of files with sum corrections
    sumAlignCorrFileListFileName = newAlignCorrFileListFileName.replace('new_', 'sum_')
    print 'sumAlignCorrFileListFileName   = '+sumAlignCorrFileListFileName
   ## store names of files with new and sum corrections into respective lists/
   #with open(newAlignCorrFileListFileName, 'w') as f :
   #    f.write(newAlignCorrFileName)
   #with open(sumAlignCorrFileListFileName, 'w') as f :
   #    f.write(sumAlignCorrFileName)

    # file for storing the plots
    if options.addInfo != '' :
        alignCorrPlotsFileName = options.digiFileListFileName.replace('digi_filelist.txt', options.addInfo+'_corr_plots.root')
    else :
        alignCorrPlotsFileName = options.digiFileListFileName.replace('digi_filelist.txt',                  'corr_plots.root')
    print 'alignCorrPlotsFileName         = '+alignCorrPlotsFileName

    newAlignCorrFileNames = []
    sumAlignCorrFileNames = []
   #newAlignCorrFileNames.append(newAlignCorrFileName+'\n')
   #sumAlignCorrFileNames.append(sumAlignCorrFileName+'\n')
    # Main loop of iterations.

    # Note that iteration numbering starts with 1,
    # --------------------------------------------

    # because when things are denoted with it01, it means that they result from
    # first round of calculations:
    for iterNr in xrange(1, options.maxNumOfIterations+1) :
        itNr = str(iterNr).rjust(2, '0') # '1' becomes '01' and so on
        # The reconstruction is run file-by-file in the following loop below,
        # and the subsequent alignment corrections are produced using the whole
        # chain of the newly produced bmndst files with the reconstructed
        # events.

        # We will need a list for storing bmndstFileName's at the current
        # iteration:
        bmndstFileNames = []

        # Run reconstruction on limited number of events from each digi file
        # from the digiFileNames list
        # counter of total events processed for the alignment:
        nEventsTotal = 0
        for digiFileName in digiFileNames :
            print 'reconstruction starting'
            print 'digiFileName                   = '+digiFileName
            # Derive bmndstFileName from digiFileListFileName,
            # adding addInfo and runNumber, extracted from digiFileName:
            # bmn_run06_Glob_tilted_beams_digi_filelist.txt  -->  'bmn_run06_Glob_tilted_beams_test_bmndst_001203_it01.root'
            # First extract the run number:
            runNumber = re.search('digi_([\d]+)', digiFileName).group(1)
            if options.addInfo != '' :
                bmndstFileName = options.digiFileListFileName.replace('digi_filelist.txt', options.addInfo+'_bmndst_'+runNumber+'_it'+itNr+'.root')
            else :
                bmndstFileName = options.digiFileListFileName.replace('digi_filelist.txt',                  'bmndst_'+runNumber+'_it'+itNr+'.root')
            print 'bmndstFileName                 = '+bmndstFileName
            # and memorise it in the list that will be used during alignment:
            bmndstFileNames.append(bmndstFileName+'\n')

            # run the reconstruction
            print 'digiFileName                   = '+digiFileName
            print 'bmndstFileName                 = '+bmndstFileName
            print 'startng event                  = '+str(0)
            print 'number of events to process    = '+str(options.nEvents)
            print 'sumAlignCorrFileName           = '+sumAlignCorrFileName
            call(['root', '-l', '-q', '$VMCWORKDIR/macro/run/run_reco_bmn.C("'+digiFileName+'", "'+bmndstFileName+'", '+str(0)+', '+str(options.nEvents)+', "'+sumAlignCorrFileName+'")'])
            # count total number of events processed for the alignment:
            nEventsTotal += options.nEvents
            print 'reconstruction ended'
        # We also prepare a file with the list of these files inside the loop.
        # It will be used to create a chain inside the determine_align_corrections_bmn.C

        # Form name of file with the list of bmndstFileName's:
        # replace digi with bmndst and add addInfo and itNr,
        # e.g. 'bmn_run06_Glob_tilted_beams_digi_filelist.txt'  -->  'bmn_run06_Glob_tilted_beams_test_bmndst_filelist_it01.txt'
        if options.addInfo != '' :
            bmndstFileListFileName = options.digiFileListFileName.replace('digi_filelist', options.addInfo+'_bmndst_filelist_it'+itNr)
        else :
            bmndstFileListFileName = options.digiFileListFileName.replace('digi_filelist',                  'bmndst_filelist_it'+itNr)
        print 'bmndstFileListFileName         = '+bmndstFileListFileName
        # create file with the list of new bmndstFileName's:
        with open(bmndstFileListFileName, 'w') as f :
            for fname in bmndstFileNames :
                f.write(fname)

        # Form name of the new alignment output file:
        # e.g. 'bmn_run06_Glob_tilted_beams_test_bmndst_filelist_it01.txt'  -->  'bmn_run06_Glob_tilted_beams_test_new_align_it01.root'
        newAlignCorrFileName = bmndstFileListFileName.replace('bmndst_filelist', 'new_align')
        newAlignCorrFileName = newAlignCorrFileName.replace('.txt',   '.root')
        print 'newAlignCorrFileName           = '+newAlignCorrFileName

        # And now run the alignment:
        print 'alignment starting'
        call(['root', '-l', '-q', '$VMCWORKDIR/macro/alignment/determine_align_corrections_bmn.C("'+bmndstFileListFileName+'", "'+newAlignCorrFileName+'", '+str(nEventsTotal)+')'])
        print 'alignment ended'
        # preserve the Millepede.res result:
        # 'bmn_run06_Glob_tilted_beams_test_new_align_it01.root'  -->  'bmn_run06_Glob_tilted_beams_test_new_align_millepede_it01.res'
        pedeResultFileName = newAlignCorrFileName.replace('align', 'align_millepede')
        pedeResultFileName =   pedeResultFileName.replace('.root', '.res')
        print 'pedeResultFileName             = '+newAlignCorrFileName
        call(['cp', 'Millepede.res', pedeResultFileName])
        # the already used for the reconstruction sumAlignCorrFileName
        # now becomes the previous one:
        preAlignCorrFileName =  sumAlignCorrFileName
        sumAlignCorrFileName =  newAlignCorrFileName.replace('new_', 'sum_')
        print 'preAlignCorrFileName           = '+preAlignCorrFileName
        print 'newAlignCorrFileName           = '+newAlignCorrFileName
        print 'sumAlignCorrFileName           = '+sumAlignCorrFileName
        if iterNr==1 and options.startAlignCorrFileName=='' : # if we started from from scratch, newAlignCorrFileName becomes also new sumAlignCorrFileName
            call(['cp', newAlignCorrFileName, sumAlignCorrFileName])
            print 'cp newAlignCorrFileName sumAlignCorrFileName'
        else :                                                # update sumAlignCorrFileName file and at next iteration use it
            print 'update_align_corrections_bmn.C starting'
            call(['root', '-l', '-q', '$VMCWORKDIR/macro/alignment/update_align_corrections_bmn.C("'+preAlignCorrFileName+'", "'+newAlignCorrFileName+'", "'+sumAlignCorrFileName+'")'])
            print 'update_align_corrections_bmn.C ended'

        newAlignCorrFileNames.append(newAlignCorrFileName+'\n')
        sumAlignCorrFileNames.append(sumAlignCorrFileName+'\n')

    # the iteration loop is finished
    # add file names with new and sum corrections to the respective lists
    with open(newAlignCorrFileListFileName, 'w') as f :
        for fname in newAlignCorrFileNames :
            f.write(fname)
    with open(sumAlignCorrFileListFileName, 'w') as f :
        for fname in sumAlignCorrFileNames :
            f.write(fname)

    # plot new and sum corrections vs. iteration number, beginning with the start values of sum corrections (at the so-called 'itertaion 0')
    call(['root', '-l', '-q', '$VMCWORKDIR/macro/alignment/plot_align_corrections_bmn.C("'+newAlignCorrFileListFileName+'")'])

if __name__ == "__main__" :
    main()
