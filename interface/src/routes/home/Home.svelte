<script lang="ts">
	import Restore from '$lib/components/RestoreComp.svelte';
	import SequencerControl from '$lib/components/SequencerControl.svelte';
	import StatusComp from '$lib/components/StatusComp.svelte';
	import WelcomeTextComp from '$lib/components/WelcomeTextComp.svelte';
</script>
<SequencerControl />
<StatusComp restPath="rest/spin-seq/diag/" />
<WelcomeTextComp />
<Restore></Restore>