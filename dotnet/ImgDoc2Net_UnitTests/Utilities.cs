using ImgDoc2Net.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ImgDoc2Net_UnitTests
{
    internal static class Utilities
    {
        public static bool IsActiveObjectCountEqual(ImgDoc2Statistics statisticsBefore, ImgDoc2Statistics statisticsAfter)
        {
            if (statisticsBefore.NumberOfCreateOptionsObjectsActive == statisticsAfter.NumberOfCreateOptionsObjectsActive &&
                statisticsBefore.NumberOfOpenExistingOptionsObjectsActive == statisticsAfter.NumberOfOpenExistingOptionsObjectsActive &&
                statisticsBefore.NumberOfDocumentObjectsActive == statisticsAfter.NumberOfDocumentObjectsActive &&
                statisticsBefore.NumberOfReader2dObjectsActive == statisticsAfter.NumberOfReader2dObjectsActive &&
                statisticsBefore.NumberOfWriter2dObjectsActive == statisticsAfter.NumberOfWriter2dObjectsActive)
            {
                return true;
            }

            return false;
        }
    }
}
