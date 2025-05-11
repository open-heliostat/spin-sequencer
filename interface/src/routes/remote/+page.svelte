<script lang="ts">
    import RemotesComp from '$lib/components/RemotesComp.svelte';

    export let data;
</script>

<RemotesComp restPath="rest/spin-seq/remotes" />