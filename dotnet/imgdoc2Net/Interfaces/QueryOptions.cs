namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    public class QueryOptions
    {
        public const int DefaultMaxNumberOfResults = 256;

        public int MaxNumbersOfResults { get; set; } = DefaultMaxNumberOfResults;

        /// <summary>   
        /// Gets or sets a value indicating whether the result was complete.
        /// This property will be set by the query-operation, and it indicates whether
        /// all results could be retrieved.
        /// </summary>
        /// <remarks>
        /// I am not expecting to be this a problem, I mean - that we provide a reasonable max number of results.
        /// If this turns out to be a problem, it would of course be possible to provide a better interop-implemantion
        /// here.
        /// </remarks>
        /// <value type="bool"> True if the result set is complete, false if not. </value>
        public bool ResultWasComplete { get; set; }
    }
}
